#!/bin/sh
set -eu

HOST=${1:-127.0.0.1}
PORT=${2:-8080}
BASE="http://$HOST:$PORT"
COOKIE_JAR=$(mktemp)

cleanup() {
  rm -f "$COOKIE_JAR"
}

trap cleanup EXIT

echo "POST /login (username=alice)..."
login_resp=$(curl -si -X POST "$BASE/login" -d 'username=alice' -c "$COOKIE_JAR")
echo "$login_resp" | sed -n '1,12p'

if ! grep -qi '^Set-Cookie: .*session_id=' <<EOF
$login_resp
EOF
then
  echo "ERROR: no session cookie received"
  exit 1
fi

echo
echo "GET /profile with cookie..."
profile_resp=$(curl -si "$BASE/profile" -b "$COOKIE_JAR")
echo "$profile_resp" | sed -n '1,12p'

if ! printf '%s' "$profile_resp" | grep -q 'Welcome, alice'; then
  echo "ERROR: profile did not accept the session cookie"
  exit 1
fi

echo
echo "GET /logout with cookie..."
logout_resp=$(curl -si "$BASE/logout" -b "$COOKIE_JAR")
echo "$logout_resp" | sed -n '1,12p'

if ! grep -qi '^Set-Cookie: session_id=;.*Max-Age=0' <<EOF
$logout_resp
EOF
then
  echo "ERROR: logout did not clear the session cookie"
  exit 1
fi

echo
echo "GET /profile after logout..."
after_logout_resp=$(curl -si "$BASE/profile" -b "$COOKIE_JAR")
echo "$after_logout_resp" | sed -n '1,12p'

if ! printf '%s' "$after_logout_resp" | grep -q '401 Unauthorized'; then
  echo "ERROR: profile still accepts the session after logout"
  exit 1
fi

echo
echo "Cookie flow test passed."

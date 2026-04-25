#!/usr/bin/env python3
import os
import sys

method = os.environ.get("REQUEST_METHOD", "GET")
body = ""
if method == "POST":
    body = sys.stdin.read()

# parser le body
params = {}
if body:
    for part in body.split("&"):
        if "=" in part:
            key, value = part.split("=", 1)
            params[key] = value.replace("+", " ")

first_name = params.get("first_name", "")
last_name = params.get("last_name", "")
username = params.get("username", "")

# page apres soumission du formulaire
if first_name and last_name and username:
    html = """Content-Type: text/html; charset=utf-8\r\n\r\n
<!DOCTYPE html>
<html>
<head><title>Profile</title>
<style>
body {{ background: #333745; color: white; font-family: sans-serif; text-align: center; padding: 50px; }}
.card {{ background: #444; padding: 30px; border-radius: 10px; display: inline-block; margin: 20px; }}
button {{ padding: 10px 20px; margin: 10px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }}
.get {{ background: #4CAF50; color: white; }}
.post {{ background: #2196F3; color: white; }}
.delete {{ background: #f44336; color: white; }}
input {{ padding: 8px; margin: 5px; border-radius: 4px; border: none; }}
#result {{ margin-top: 20px; padding: 15px; background: #555; border-radius: 5px; min-height: 40px; }}
</style>
</head>
<body>
<h1>Welcome, {first_name} {last_name} (@{username})</h1>

<div class="card">
    <h2>GET &mdash; Fetch a file</h2>
    <input type="text" id="get_path" placeholder="/upload_file/test.txt" />
    <br>
    <button class="get" onclick="doGet()">GET</button>
</div>

<div class="card">
    <h2>POST &mdash; Upload a file</h2>
    <input type="file" id="upload_file" />
    <br>
    <button class="post" onclick="doPost()">Upload</button>
</div>

<div class="card">
    <h2>DELETE &mdash; Delete a file</h2>
    <input type="text" id="delete_path" placeholder="/upload_file/test.txt" />
    <br>
    <button class="delete" onclick="doDelete()">DELETE</button>
</div>

<div id="result">Results will appear here...</div>

<script>
function doGet() {{
    var path = document.getElementById("get_path").value;
    fetch(path, {{ method: "GET" }})
        .then(r => r.text())
        .then(t => document.getElementById("result").innerText = "GET OK: " + t.substring(0, 200))
        .catch(e => document.getElementById("result").innerText = "Error: " + e);
}}

function doPost() {{
    var file = document.getElementById("upload_file").files[0];
    if (!file) {{ document.getElementById("result").innerText = "No file selected"; return; }}
    var formData = new FormData();
    formData.append("file", file);
    fetch("/upload_file/", {{ method: "POST", body: formData }})
        .then(r => r.text())
        .then(t => document.getElementById("result").innerText = "POST OK: file uploaded")
        .catch(e => document.getElementById("result").innerText = "Error: " + e);
}}

function doDelete() {{
    var path = document.getElementById("delete_path").value;
    fetch(path, {{ method: "DELETE" }})
        .then(r => r.text())
        .then(t => document.getElementById("result").innerText = "DELETE OK: " + path + " deleted")
        .catch(e => document.getElementById("result").innerText = "Error: " + e);
}}
</script>
</body>
</html>""".format(first_name=first_name, last_name=last_name, username=username)

else:
    # page formulaire initiale
    html = """Content-Type: text/html; charset=utf-8\r\n\r\n
<!DOCTYPE html>
<html>
<head><title>Login</title>
<style>
body {{ background: #333745; color: white; font-family: sans-serif; text-align: center; padding: 50px; }}
input {{ padding: 10px; margin: 8px; border-radius: 5px; border: none; width: 200px; }}
button {{ padding: 10px 30px; background: #2196F3; color: white; border: none; border-radius: 5px; cursor: pointer; }}
</style>
</head>
<body>
<h1>Webserv Login</h1>
<form method="POST" action="/cgi-bin/forms.py">
    <input type="text" name="first_name" placeholder="First name" required /><br>
    <input type="text" name="last_name" placeholder="Last name" required /><br>
    <input type="text" name="username" placeholder="Username" required /><br>
    <button type="submit">Login</button>
</form>
</body>
</html>"""

sys.stdout.write(html)
sys.stdout.flush()

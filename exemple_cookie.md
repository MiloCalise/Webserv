# Creer un compte:

---

```bash
    curl -i -X POST http://localhost:8080/login -d username=test_user
```

# Se connecter au compte avec le session_id
```bash
    curl -i -X GET http://localhost:8080/profile -H "Cookie: session_id=ta session_id enregistrer juste avant"
```

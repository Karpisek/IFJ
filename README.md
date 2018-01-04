# IFJ projekt

* Roman Zajíc (xzajic16)
* Jakub Trubka (xtrubk00)
* Miroslav Karpíšek (xkarpi05)
* Jakub Samek (xsamek07)

---

## Návody

### Github

* __dev__ je základní branch, jakoby master, dávat pozor na to kde se nacházíte, nejdříve zkontrolovat než se začne kodit

### Pravidla
* pouzivani camel case pro nazvy promenych (myVeryVeryLongVariable)
* styl funkci
```
void mojeFunkce() {
	...
}
```
* komentare v cestine bez diakritiky

---
Tohle asi pak smazat

1. Create new branch (z __dev__ branch), např prijmeni_num
2. Publish branch
3. Editace souboru (říkat kdo kam kdy sahá aby se nestalo že více lidí sahá na stejný soubor)
4. Save
5. Commit to branch
6. Push origin
7. Create Pull Request -> budete přesměrování na web interface (Míra z Atomu *god*)
8. Create Pull Request, Merge do __dev__ branche
9. Říct o tom
10. Každý u sebe Fetch origin a Pull origin (updatnou se soubory)

Po mergi už nepracujte v té branchi a udělejte si opět novou.

- dev branch je místo na testování commitů/mergů, úspěšné testování => může se udělat merge do masteru, v masteru vždy jen funkční kód

# Migrate to GitHub Actions + prepare for Library Manager

**Summary**
- Remove Travis CI config and badge.
- Add GitHub Actions:
  - `arduino-lint` (strict + Library Manager submit checks).
  - Compile all sketches in `examples/` for UNO, Leonardo, Mega via `arduino/compile-sketches`.
  - Optional CodeQL job for C/C++ analysis.
- Refresh `README.md` (badges, install, release flow).
- Normalize `library.properties` for Library Manager.
- Expand `keywords.txt` for IDE syntax highlighting.

**Notes**
- After merge, tag `v0.1.0` and submit the repo URL to `arduino/library-registry`.
- Extend the build matrix to more boards any time.

**Checklist**
- [ ] Delete `.travis.yml` (removed in this PR)
- [ ] Confirm maintainer email in `library.properties` (currently just name)
- [ ] CI green

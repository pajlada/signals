# Changelog

## Unreleased

## v0.1.1 - 2026-02-01

- Dev: Added an install target for easier Linux system integration. (#56)

## v0.1.0 - 2023-06-10

- Breaking: Move ScopedConnection to its own file `pajlada/signals/scoped-connection.hpp`. (#15)
- Breaking: Heavily restrict ScopedConnection, forcing users to store it in a `unique_ptr` instead (or use `SignalHolder` as the helper). (#18)
- Minor: Add the `clear` method to `SignalHolder`. (#20)
- Fix: SignalHolder not being able to be used in pajlada/settings UserManagedConnectionManager. (#19)
- Fix: Removed unused connection-index. (#26)
- Dev: Add test for `SignalHolder`. (#17)
- Dev: Categorize tests by splitting them up into separate files. (#13)
- Dev: Update version of googletest. (#14)

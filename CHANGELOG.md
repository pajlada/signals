# Changelog

## Unreleased

- Breaking: Move ScopedConnection to its own file `pajlada/signals/scoped-connection.hpp`. (#15)
- Breaking: Heavily restrict ScopedConnection, forcing users to store it in a `unique_ptr` instead (or use `SignalHolder` as the helper). (#18)
- Dev: Add test for `SignalHolder`. (#17)
- Dev: Categorize tests by splitting them up into separate files. (#13)
- Dev: Update version of googletest. (#14)

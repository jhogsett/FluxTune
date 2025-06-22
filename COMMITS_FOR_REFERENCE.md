# Commits Preserved for Reference

This document lists commits that were removed from `main` but preserved in the `backup-commits-before-reset` branch for future reference or cherry-picking.

## Reset Information
- **Known Good State**: 3b0198c (station lock LED and contrast fixes working)
- **Backup Branch**: `backup-commits-before-reset`
- **Reset Date**: June 21, 2025

## Preserved Commits (newest to oldest)

### dc68b1b - Add signal meter anomaly documentation and clean up device variant config
- Contains documentation updates
- May have useful device variant configuration cleanup
- **Status**: Review for potential documentation improvements

### 5022185 - Merge PR #22: Add CW station drift and random callsign features  
- Contains CW station drift functionality
- Random callsign generation features
- **Status**: May want to cherry-pick drift features later

### a5af7f1 - WIP: Attempted panel LED revert - need to go back to working state
- Work-in-progress attempt to fix panel LED issues
- **Status**: Likely not needed (we have working state at 3b0198c)

### eeea50f - added cw station drift and round mgmt
- CW station drift implementation
- Round management features  
- **Status**: May want to cherry-pick drift features later

## How to Access These Commits

```bash
# Switch to backup branch to examine commits
git checkout backup-commits-before-reset

# View specific commit
git show <commit-hash>

# Cherry-pick a specific commit (from main branch)
git cherry-pick <commit-hash>

# Create a new feature branch from a specific commit
git checkout -b feature/drift-features <commit-hash>
```

## Next Steps
- Main branch reset to known good state 3b0198c
- Station lock LED and contrast fixes confirmed working
- CW drift features available for future integration from backup branch

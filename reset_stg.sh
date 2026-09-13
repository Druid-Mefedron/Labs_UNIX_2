#!/bin/bash
set -e

git checkout stg
git fetch origin
git reset --hard origin/stg
git clean -fd

echo "Reset stg to origin/stg, removed uncommitted changes and untracked files"

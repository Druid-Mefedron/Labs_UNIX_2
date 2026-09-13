#!/bin/bash
set -e

git checkout prd
git fetch origin
git reset --hard origin/prd
git clean -fd

echo "Reset prd to origin/prd, removed uncommitted changes and untracked files"

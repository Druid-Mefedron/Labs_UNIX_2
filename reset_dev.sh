#!/bin/bash
set -e

git checkout dev
git fetch origin
git reset --hard origin/dev
git clean -fd

echo "Reset dev to origin/dev, removed uncommitted changes and untracked files"

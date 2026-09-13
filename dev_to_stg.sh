#!/bin/bash
set -e

git checkout stg
git merge dev --no-ff -m "merge dev into stg"
TAG_NAME="stg-$(date +%Y%m%d-%H%M%S)"
git tag "$TAG_NAME"
git push origin stg
git push origin "$TAG_NAME"
echo "Merged dev into stg, tagged as $TAG_NAME"

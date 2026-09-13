#!/bin/bash
set -e
git checkout prd
git merge stg --no-ff -m "merge stg into prd"

TAG_NAME="prd-$(date +%Y%m%d-%H%M%S)"
git tag "$TAG_NAME"
git push origin prd
git push origin "$TAG_NAME"
echo "Merged stg into prd, tagged as $TAG_NAME"

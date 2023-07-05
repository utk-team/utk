#!/bin/sh

python3 -m mkdocs build
git add docs/
git commit -m "Update documentation"
git push
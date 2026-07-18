#!/usr/bin/env bash
# Create (or reuse) a PR for staged screenshot changes and squash-merge it.
set -euo pipefail

BRANCH=""
TITLE=""
BODY=""
BASE="master"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --branch) BRANCH="$2"; shift 2 ;;
    --title) TITLE="$2"; shift 2 ;;
    --body) BODY="$2"; shift 2 ;;
    --base) BASE="$2"; shift 2 ;;
    *) echo "Unknown arg: $1" >&2; exit 2 ;;
  esac
done

if [[ -z "$BRANCH" || -z "$TITLE" ]]; then
  echo "usage: $0 --branch NAME --title TITLE [--body TEXT] [--base master]" >&2
  exit 2
fi

git config user.name "github-actions[bot]"
git config user.email "41898282+github-actions[bot]@users.noreply.github.com"

if git diff --staged --quiet; then
  echo "skipped=true" >> "${GITHUB_OUTPUT:-/dev/null}"
  echo "No staged screenshot changes."
  exit 0
fi

git commit -m "$TITLE"
git push -u origin "$BRANCH" --force

PR_URL=$(gh pr list --head "$BRANCH" --json url --jq '.[0].url // empty')
if [[ -z "$PR_URL" ]]; then
  PR_URL=$(gh pr create --title "$TITLE" --body "${BODY:-$TITLE}" --base "$BASE" --head "$BRANCH")
fi

echo "PR: $PR_URL"
echo "pr_url=$PR_URL" >> "${GITHUB_OUTPUT:-/dev/null}"
echo "skipped=false" >> "${GITHUB_OUTPUT:-/dev/null}"

# GITHUB_TOKEN pushes do not re-trigger PR checks; merge when mergeable.
gh pr merge --auto --squash "$PR_URL" || true
gh pr merge --squash --delete-branch "$PR_URL" || true

for _ in $(seq 1 36); do
  STATE=$(gh pr view "$PR_URL" --json state --jq .state)
  if [[ "$STATE" == "MERGED" ]]; then
    echo "Merged $PR_URL"
    exit 0
  fi
  sleep 5
done

echo "Timed out waiting for merge of $PR_URL" >&2
exit 1

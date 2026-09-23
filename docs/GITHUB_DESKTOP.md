# Publishing with GitHub Desktop

This prepared repository is intended to live locally at:

```text
D:\Github\YAMuino
```

Recommended remote repository:

```text
https://github.com/johnlenfr/YAMuino
```

## 1. Copy the prepared folder

Extract or copy the complete `YAMuino` folder into:

```text
D:\Github\YAMuino
```

Confirm that this file exists:

```text
D:\Github\YAMuino\YAMuino.ino
```

Do not move the contents one level deeper into another `YAMuino` directory.

## 2. Add the folder in GitHub Desktop

In GitHub Desktop:

1. Choose **File -> Add local repository...**
2. Select `D:\Github\YAMuino`.
3. If GitHub Desktop reports that the folder is not yet a Git repository, use the offered **create a repository** option for this folder.
4. Use `YAMuino` as the repository name.
5. Do not ask GitHub Desktop to generate another README, `.gitignore`, or license because those choices are already handled by this package.

## 3. Review ignored files

The local folder `reference-local/` contains the supplied Yamaha service manual for your personal technical reference. It is excluded by `.gitignore` and should not appear in the list of files to commit.

This is intentional so that the third-party manual is not automatically republished.

## 4. Create the first commit

Review the files shown under **Changes**.

Suggested commit message:

```text
Initial YAMuino release
```

Then choose **Commit to main**.

If GitHub Desktop initially uses a branch named something other than `main`, you may rename the default branch to `main` before publication.

## 5. Publish to GitHub

Choose **Publish repository** and use:

- **Name:** `YAMuino`
- **Owner:** `johnlenfr`
- **Description:** `Arduino UNO IR bridge for Yamaha CDX-E410 System Control`
- **Visibility:** choose Public or Private according to your preference

For a public repository, make sure the option that keeps the code private is not selected.

After publication, the expected URL is:

```text
https://github.com/johnlenfr/YAMuino
```

## 6. Verify the published repository

On GitHub, check that:

- `README.md` renders on the repository front page;
- `YAMuino.ino` is visible at the root;
- the `docs/` directory is present;
- the GitHub Actions workflow starts;
- `reference-local/` is **not** present;
- the README build badge becomes valid after the first successful workflow run.

## 7. Normal update workflow

For later changes:

1. Edit files in `D:\Github\YAMuino`.
2. Open GitHub Desktop.
3. Review the diff.
4. Enter a concise commit summary.
5. Commit to `main` or to a feature branch.
6. Click **Push origin**.

For larger changes, create a branch first and merge it through a pull request on GitHub.

## If you choose another repository name

The Arduino IDE expects the primary `.ino` file and sketch folder name to match cleanly. If you rename the local project folder, also consider renaming `YAMuino.ino`. You should also update the GitHub badge and repository links in `README.md`.

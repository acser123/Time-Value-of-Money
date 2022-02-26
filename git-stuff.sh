# ssh-keygen -t ed25519 -C "acser@yahoo.com"
# eval "$(ssh-agent -s)"
# ssh-add ~/.ssh/id_ed25519
# cat ~/.ssh/id_ed25519.pub
#  Add the above to the Github user settings per https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account

# Blow away git directory
# rm -fr ./.git

# Initialize git
# git init

# This will create a local directory called Time-Value-of-Money
git clone https://github.com/acser123/Time-Value-of-Money

# Check connectivity
ssh -T git@github.com

# Local SSH key used as credential to log into github
git remote set-url origin git@github.com:acser123/Time-Value-of-Money.git

# Add local files
git add *

# Commit
git commit -m "any comment here"

# Push
git push

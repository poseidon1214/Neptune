# Usage: rollback.sh revision_number.
# Say you want to rollback revision 12345, run this script in BLADE_ROOT
# directory:
#
# rollback.sh 12345
#
# If everything is ok, you may upload rollback change to reviewer now.

revision=$1
svn merge -r $revision:$(($revision-1)) .

# Usage: cherry_pick.sh revision_number.
# Say you want to cherry pick revision 12345 of gdt in trunk to tag:
#
# cd $BLADE_ROOT_OF_TAG
# cherry_pick.sh 12345
#
# It will patch diff of 12344 and 12345 to current directory.
# If everything is ok, you may upload cherry pick change to reviewer now.

revision=$1
svn merge -r $(($revision-1)):$revision http://tc-svn.tencent.com/isd/isd_qzonebrand_rep/qzone_adsdev_proj/trunk


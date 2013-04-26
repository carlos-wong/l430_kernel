#!/bin/bash

# Checkout mid-root from svn
#echo "Checkout mid-root from svn, rename 'mid-root-release'..."
#svn co -q svn://192.168.1.22/system/linux/05root/mid-root/trunk mid-root-release 
#cd mid-root-release || exit 1
#echo "Done"


#DOC_FILES="packages"

#echo "delete : \"${DOC_FILES}\""
#rm -fr ${DOC_FILES}




### Delete include
#echo "Delete include ..."
#find -name "include" 2>/dev/null | xargs -i rm -fr {}
#echo "Done"

### Delete man
#echo "Delete man ..."
#find -name "man"  2>/dev/null | xargs -i rm -fr {}
#echo "Done"

### Delete doc
#echo "Delete doc ..."
#find -name "*doc"  2>/dev/null | xargs -i rm -fr {}
#echo "Done"


### delete lib*.a 
#echo -n "Delete lib*.a ..."
#find -name "*.a"  2>/dev/null | xargs -i rm -fr {}
#echo "Done"

### delete .svn  
echo -n "Delete .svn ... "
find -name ".svn"  2>/dev/null | xargs -i rm -fr {}
echo "Done"


#comment() {
### Reduce themes
#echo "Delete themes..."
#mv usr/share/themes usr/share/themes-all
#mkdir usr/share/themes 
#cp -a usr/share/themes-all/Default usr/share/themes-all/Raleigh usr/share/themes
#rm -fr usr/share/themes-all
#echo "Done"


### Delete locale
#mv usr/share/locale usr/share/locale-all
#mkdir usr/share/locale 
#cp -a usr/share/locale-all/zh_CN usr/share/locale 
#rm -fr usr/share/locale-all

### Delete icons
echo "Delete icons"
#rm -fr usr/share/icons/Rodent

} ### comment()



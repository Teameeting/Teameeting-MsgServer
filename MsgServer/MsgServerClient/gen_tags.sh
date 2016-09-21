#!/bin/sh

## generate system header tags
##ctags -I __THROW --file-scope=yes --langmap=c:+.h --languages=c,c++ --links=yes --c-kinds=+p --fields=+S  -R -f ~/.vim/systags /usr/include/c++/4.8.2
## set tags+=~/.vim/systags

#### generate tags
cp gen_tags.sh .gen_tags.sh
ctags -R --c++-kinds=+px --fields=+iaS --extra=+q .

#!/bin/bash
HOST='sftp://web.sourceforge.net'
USER='jdavidberger'

TARGETFOLDER='/home/project-web/rikitiki/htdocs/doxygen'
SOURCEFOLDER='/home/justin/rikitiki/bin/doc/html'

lftp -f "
open $HOST 
user $USER 
lcd $SOURCEFOLDER 
mirror --reverse --delete --verbose $SOURCEFOLDER $TARGETFOLDER \
bye \
"


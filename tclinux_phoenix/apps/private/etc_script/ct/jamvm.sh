SUPER_ACCOUNT=`/userfs/bin/tcapi get Account_Entry0 username`
if [ "$SUPER_ACCOUNT" = "" ] ;then
	SUPER_ACCOUNT=CMCCAdmin
fi
rm -rf /tmp/felix-cache
su -p $SUPER_ACCOUNT && export USER=admin && cd /usr/osgi/felix-framework && /usr/local/jre/bin/java -jamvm -Djava.security.policy=all.policy -Dorg.osgi.framework.security="osgi" -Djava.library.path=/lib/ -Dfile.encoding=UTF-8 -jar bin/felix.jar &

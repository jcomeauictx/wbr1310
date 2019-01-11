#

all:

install:
	cp -ra ./config		$(TARGET)/etc/config
	cp config.sh		$(TARGET)/etc/scripts/.
	cp set_nodes.sh		$(TARGET)/etc/scripts/.
	cp set_nodes.php	$(TARGET)/etc/scripts/.
	cp system.sh		$(TARGET)/etc/scripts/.
	cp layout_run.php	$(TARGET)/etc/scripts/.
	cp layout.sh		$(TARGET)/etc/scripts/.
	cp setext.sh		$(TARGET)/etc/scripts/.
	cp enlan.sh		$(TARGET)/etc/scripts/.
	chmod +x		$(TARGET)/etc/scripts/*.sh
	#
	ln -sf /etc/scripts/config.sh $(TARGET)/etc/init.d/S03config.sh
	ln -sf /etc/scripts/system.sh $(TARGET)/etc/init.d/S10system.sh
	#
	cp haltdemand.sh	$(TARGET)/etc/scripts/misc/.
	cp preupgrade.sh	$(TARGET)/etc/scripts/misc/.
	#
	cp cabletest:?		$(TARGET)/usr/sbin/.
	cp switch		$(TARGET)/usr/sbin/.
	cp setwantype.sh	$(TARGET)/etc/scripts/misc/.

clean:

.PHONY: all install clean

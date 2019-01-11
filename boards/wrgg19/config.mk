#
# Board dependent Makefile for WRG-G19
#

KERNEL_IMG:=kernel.img
ROOTFS_IMG:=rootfs.img
TFTPIMG   :=tftp_$(IMAGE_SIGNATURE).bin
MYNAME    :=WRG-G19

Q?=@

#############################################################################

remove_fsimg:
	$(Q)rm -f $(ROOTFS_IMG)

.PHONY: remove_fsimg

#############################################################################
#
# build root file system image
#
squashfs: clean_CVS remove_fsimg
	@echo -e "\033[32m$(MYNAME) make squashfs!\033[0m"
	$(Q)make -C ./tools/squashfs-tools
	$(Q)./tools/squashfs-tools/mksquashfs userland/target $(ROOTFS_IMG) -be

squashfs_lzma: clean_CVS remove_fsimg
	@echo -e "\033[32m$(MYNAME) make squashfs (LZMA)!\033[0m"
	$(Q)make -C ./tools/squashfs-tools
	$(Q)./tools/squashfs-tools/mksquashfs-lzma userland/target $(ROOTFS_IMG) -be

.PHONY: squashfs squashfs_lzma

#############################################################################
#
# build kernel image
#
kernel_image:
	@echo -e "\033[32m$(MYNAME) Building image file ...\033[0m"
	$(Q)rm -f $(KERNEL_IMG)
	$(Q)mips-linux-objcopy -O binary -g $(KERNELDIR)/vmlinux vmlinux.bin
	$(Q)gzip vmlinux.bin
	$(Q)mv vmlinux.bin.gz $(KERNEL_IMG)

kernel_image_lzma:
	@echo -e "\033[32m$(MYNAME) Building image file (LZMA)...\033[0m"
	$(Q)rm -f vmlinux.bin $(KERNEL_IMG)
	$(Q)make -C ./tools/squashfs-tools
	$(Q)mips-linux-objcopy -O binary -g $(KERNELDIR)/vmlinux vmlinux.bin
	$(Q)./tools/squashfs-tools/lzma/lzma e vmlinux.bin $(KERNEL_IMG)
	$(Q)rm -f vmlinux.bin

.PHONY: kernel_image kernel_image_lzma

##########################################################################
#
# Common target
#

kernel:
	@echo -e "\033[32m$(MYNAME) make kernel!\033[0m"
	$(Q)make -C kernel mrproper
	$(Q)cp kernel/wrgg19.config kernel/.config
	$(Q)make -C kernel oldconfig
	$(Q)make -C kernel clean
	$(Q)make -C kernel dep
	$(Q)make -C kernel

release: kernel_image_lzma squashfs_lzma
	@echo -e "\033[32m$(MYNAME) make F/W image (release image)!\033[0m"
	$(Q)[ -d images ] || mkdir -p images
	$(Q)make -C tools/buildimg
	$(Q)./tools/buildimg/packimgs -v -o raw.img -i $(KERNEL_IMG) -i $(ROOTFS_IMG)
	$(Q)./tools/buildimg/v2image -v -i raw.img -o images/web.img -d /dev/mtdblock/2 -s $(IMAGE_SIGNATURE) -c -b
	$(Q)rm -f raw.img
	$(Q)mv images/web.img images/$(IMAGE_SIGNATURE).`cat userland/buildno`.bin
	@echo -e "\033[32m--------------------------------------------------------\033[0m"
	@echo -e "\033[32mSIGNATURE    : \033[0m"$(IMAGE_SIGNATURE)
	@echo -e "\033[32mBUILD NUMBER : \033[0m"`cat userland/buildno`
	@echo -e "\033[32m--------------------------------------------------------\033[0m"
	$(Q)ls -al images/$(IMAGE_SIGNATURE).`cat userland/buildno`.bin

magic_release: kernel_image_lzma squashfs_lzma
	@echo -e "\033[32m$(MYNAME) make F/W image (release image)!\033[0m"
	$(Q)[ -d images ] || mkdir -p images
	$(Q)make -C tools/buildimg
	$(Q)./tools/buildimg/packimgs -v -o raw.img -i $(KERNEL_IMG) -i $(ROOTFS_IMG)
	$(Q)./tools/buildimg/v2image -v -i raw.img -o images/web.img -d /dev/mtdblock/2 -s wrgg19_aLpHa -c -b
	$(Q)rm -f raw.img
	$(Q)mv images/web.img images/$(IMAGE_SIGNATURE).`cat userland/buildno`.magic.bin
	@echo -e "\033[32m--------------------------------------------------------\033[0m"
	@echo -e "\033[32mSIGNATURE    : \033[0m"$(IMAGE_SIGNATURE)
	@echo -e "\033[32mBUILD NUMBER : \033[0m"`cat userland/buildno`
	@echo -e "\033[32m--------------------------------------------------------\033[0m"
	$(Q)ls -al images/$(IMAGE_SIGNATURE).`cat userland/buildno`.magic.bin

tftpimage: kernel_image_lzma squashfs_lzma
	@echo -e "\033[32m$(MYNAME) make F/W image (tftp image)!\033[0m"
	$(Q)make -C tools/buildimg
	$(Q)./tools/buildimg/packimgs -v -o raw.img -i $(KERNEL_IMG) -i $(ROOTFS_IMG)
	$(Q)mv raw.img $(TFTPIMG)
	@echo -e "\033[32m--------------------------------------------------------\033[0m"
	@echo -e "\033[32mSIGNATURE    : \033[0m"$(IMAGE_SIGNATURE)
	@echo -e "\033[32mBUILD NUMBER : \033[0m"`cat userland/buildno`
	@echo -e "\033[32mTFTP IMAGE   : \033[0m"$(TFTPIMG)
	@echo -e "\033[32m--------------------------------------------------------\033[0m"
	@echo -e "\033[32mCopy $(TFTPIMG) to /tftpboot/$(USER)/.\033[0m"
	$(Q)[ -d /tftpboot/$(USER) ] || mkdir -p /tftpboot/$(USER)
	$(Q)cp -f $(TFTPIMG) /tftpboot/$(USER)/.
	$(Q)ls -al $(TFTPIMG)

.PHONY: kernel release tftpimage

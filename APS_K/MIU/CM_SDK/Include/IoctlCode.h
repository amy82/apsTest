///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 by PERI ETG Painkiller
// All rights reserved
//
// File Name		: IoctlCode.h
// Written By		: Painkiller
//
// Revision History	
// Rev.  Date				Who 			  Description
// 0.1  2006. 11. 3			Painkiller
//



#ifndef _MPORT_IOCTL_CODE_
#define _MPORT_IOCTL_CODE_

// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
//
// FILE_DEVICE_UNKNOWN

// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//
#define 	ISD_PCI_VER					0x12022302
////////	Base codes for PCI Express Interface card
#define		IOCTLK_GET_DRV_VER  		0x00 
#define		IOCTLK_INDEX_WRITE_DATA		0x01
#define 	IOCTLK_INDEX_READ_DATA		0x02
#define 	IOCTLK_AOOC_MEM				0x03
#define		IOCTLK_FREE_MEM				0x04
#define		IOCTLK_INTR_TR				0x05
#define		IOCTLK_INTR_FAL				0x06
#define		IOCTLK_BUSMST_REQ			0x07
#define		IOCTLK_READ_PCI_REG			0x08
#define		IOCTLK_ACT_DONE_CHK			0x09

#define		IOCTLK_MEM_INIT				0x0a
#define		IOCTLK_GET_APPID			0x0b
#define		IOCTLK_SET_APPID			0x0c
#define		IOCTLK_CHK_RCVDONE			0x0e
#define		IOCTLK_CHK_PHYMEM			0x0f

//			New Enahanced Nwrite Sequence
#define		IOCTLK_PHYMEM_COPY			0x10
#define		IOCTLK_NWRITE_NEW			0x11
#define		IOCTLK_PHYMEM_COPYBACK		0x12
#define		IOCTLK_PHYMEM_COPYBACK_SAFE	0x14
#define		IOCTLK_I2C_CHKDONE			0x13
#define		IOCTLK_SET_USERMEM			0x1d
#define		IOCTLK_CLR_USERMEM			0x1e


////////	SRIO Support Routines
#define		IOCTLK_SRIO_ADD_LUT_ENTRY	0x30
#define		IOCTLK_SRIO_BASE_DEVID_RD	0x31
#define		IOCTLK_SRIO_BASE_DEVID_WR	0x32
#define		IOCTLK_SRIO_CONFIG_RD		0x33
#define		IOCTLK_SRIO_CONFIG_WR		0x34
#define		IOCTLK_SRIO_GET_INIT_RESP	0x35
#define		IOCTLK_SRIO_INIT_LUT		0x36
#define		IOCTLK_SRIO_LINK_STAT_RD	0x37
#define		IOCTLK_SRIO_NREAD			0x38
#define		IOCTLK_SRIO_NWRITE			0x39
#define		IOCTLK_SRIO_PCI_SRIO_RST	0x3a
#define		IOCTLK_SRIO_PEER_RST		0x3b
#define		IOCTLK_RD_PORT_ERR_STAT		0x3c
#define		IOCTLK_SRIO_NREADM			0x3d
#define		IOCTLK_SEND_DOORBELL		0x3e
#define		IOCTLK_SEND_MESSAGE			0x3f
////////	ETC.
#define		IOCTLK_TIME_OUT_SET			0x40
#define		IOCTLK_TIME_OUT_GET			0x41
#define		IOCTLK_TEST1				0x42
#define		IOCTLK_TEST2				0x43
#define		IOCTLK_SEND_USRRET			0x44
#define		IOCTLK_WR_ENABLE			0x4b
#define		IOCTLK_WR_DISABLE			0x4c
#define		IOCTLK_WR_SETRST			0x4d
#define		IOCTLK_WR_CLRRST			0x4e


// User Memory Sect

#define		IOCTLK_SET_USER_VADDR		0x80	// METHOD_DIRECT를 사용해서 유저메모리의 mdl을 생성하도록
#define		IOCTLK_CLR_USER_VADDR		0x81

////////	Base codes for PCI Express Interface card V2
#define 	IOCTL_MPORT_GET_DRV_VER		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_GET_DRV_VER, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define 	IOCTL_WRITE_DATA			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_INDEX_WRITE_DATA, 	METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define 	IOCTL_READ_DATA				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_INDEX_READ_DATA, 	METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define 	IOCTL_ALLOCATE_MEMORY		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_AOOC_MEM, 			METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define 	IOCTL_FREE_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_FREE_MEM, 			METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define 	IOCTL_INTR_TRUE				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_INTR_TR, 			METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define 	IOCTL_INTR_FALSE			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_INTR_FAL, 			METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_BUSMAST_REQUEST		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_BUSMST_REQ, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_READ_PCI_REG			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_READ_PCI_REG, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_ACT_DONE_CHK			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_ACT_DONE_CHK, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_MEM_INIT				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_MEM_INIT, 			METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_GET_APPID				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_GET_APPID, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_SET_APPID				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SET_APPID, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_SET_USERMEM			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SET_USERMEM, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_CLR_USERMEM			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_CLR_USERMEM, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)

#define		IOCTL_CHK_RCVDONE			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_CHK_RCVDONE, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_CHK_PHYMEM			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_CHK_PHYMEM, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)

#define		IOCTL_PHYMEM_COPY			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_PHYMEM_COPY, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_PHYMEM_COPYBACK		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_PHYMEM_COPYBACK, 	METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_PHYMEM_COPYBACK_SAFE	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_PHYMEM_COPYBACK_SAFE, 	METHOD_BUFFERED, 	FILE_ANY_ACCESS)

#define		IOCTL_NWRITE_NEW			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_NWRITE_NEW, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)
#define		IOCTL_I2C_CHKDONE			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_I2C_CHKDONE, 		METHOD_BUFFERED, 	FILE_ANY_ACCESS)

////////	End of Base codes for PCI Express Interface card
#define		IOCTL_SRIO_ADD_LUT_ENTRY	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_ADD_LUT_ENTRY  	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_BASE_DEVID_RD	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_BASE_DEVID_RD  	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_BASE_DEVID_WR	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_BASE_DEVID_WR  	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_CONFIG_RD		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_CONFIG_RD	    	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_CONFIG_WR		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_CONFIG_WR	    	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_GET_INIT_RESP	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_GET_INIT_RESP 	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_INIT_LUT			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_INIT_LUT			, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_LINK_STAT_RD		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_LINK_STAT_RD		, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_NREAD			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_NREAD		    	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_NWRITE			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_NWRITE			, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_PCI_SRIO_RST		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_PCI_SRIO_RST		, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_PEER_RST			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_PEER_RST			, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_RD_PORT_ERR_STAT		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_RD_PORT_ERR_STAT		, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SRIO_NREADM			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SRIO_NREADM		   	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SEND_DOORBELL			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SEND_DOORBELL		   	, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_SEND_MESSAGE			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SEND_MESSAGE		   	, METHOD_BUFFERED, FILE_ANY_ACCESS)

////////	End of SRIO Support Routines

#define		IOCTL_TIME_OUT_SET			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_TIME_OUT_SET, 	METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_TIME_OUT_GET			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_TIME_OUT_GET, 	METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_TEST					CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_TEST1, 		METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_TEST2					CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_TEST2, 		METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_USER_RESET			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SEND_USRRET, 	METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_WR_ENABLE				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_WR_ENABLE, 	METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_WR_DISABLE			CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_WR_DISABLE, 	METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_WR_SETRST				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_WR_SETRST, 	METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_WR_CLRRST				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_WR_CLRRST, 	METHOD_BUFFERED, FILE_ANY_ACCESS)    
////////	End of ETC.
#define		IOCTL_SET_USER_VADDR		CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTLK_SET_USER_VADDR, 	METHOD_DIRECT, FILE_ANY_ACCESS)    







#endif// _MPORT_IOCTL_CODE_
 
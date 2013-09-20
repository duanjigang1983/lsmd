#include "NetHelper.h"
#ifdef __LINUX__
int	get_nic_list(nic_t *list)
/*
{ 
	int nRet = 0;
	typedef struct 
	{
		char name[30];
		unsigned int addr;
	}nic_name_t;
	nic_name_t nic_list[16];
	unsigned int nic_num = 0;
	unsigned int index = 0;
	const char* szfile = "/proc/net/dev";
	FILE * fp = fopen (szfile, "r");
	char szline[1024] = {0};
	if (!fp) return -1;
	
	while (fgets(szline, 1024, fp))
	{
		char* pos = strchr (szline, ':');
		char * p = szline;
		
		if (!pos) continue;
		*pos = '\0';
		while(isspace(*p) && (p != pos)) p++;
		if (p == pos) continue;
		if (strncmp(p, "lo", 2) == 0) continue;
		//printf ("%s\n", p);
		memset (nic_list + nic_num, 0, sizeof(nic_name_t));
		strcpy(nic_list[nic_num].name, p);
		
		nic_num++;
	}
	fclose (fp);
	
	struct ifreq	ifr;
	int	num;
	num = 0;
    	int packet_socket = socket(AF_INET, SOCK_PACKET, htons(0x3));
    	if  (packet_socket<0)
    	{   
		return -1;
   	}

	for (index = 0; index < nic_num; index++)
	{
		//printf ("%s\n", nic_list[index].name);	
    		char szip[20] = {0};
		strncpy( ifr.ifr_name, nic_list[index].name, 14);
    		int on = ioctl( packet_socket, SIOCGIFADDR, &ifr );
		if(on == 0)
		{
			nic_list[index].addr = *(unsigned int *)&ifr.ifr_ifru.ifru_addr.sa_data[2];	
			STR_IP(szip,  nic_list[index].addr);
			list[nRet] = nic_list[index].addr;
			nRet++;
			//printf ("%s----%s--%u\n", nic_list[index].name, szip, nic_list[index].addr);
		}else nic_list[index].addr = 0;
	}
	close (packet_socket);
	return nRet;
	}
*/
{
	int _sock;
	int nRet = 0;
	int i = 0;
	struct ifreq _ifr;
	struct ifconf _ifc;
	int n;
	struct  ifreq   *pifr = 0;
	nic_t _device;
	_sock = socket( AF_INET, SOCK_DGRAM, 0 );
	if (_sock <= 0)
	{
		printf ("socket failed\n");
		 return -1;
	}
	bzero( &_ifc, sizeof(_ifc) );
	// first, you must get number of net device which you have
	// ioctl(SIOCGIFCONF) with zero(ifc) will return the length
	int	ret = ioctl( _sock, SIOCGIFCONF, &_ifc, sizeof(_ifc) );
	if (ret != 0)
	{
		printf ("ioclt SIOCGIFCONF, failed:%d:%d\n", ret, _sock);
	 	return -1;
	}//assert( !ret );

	// must alloc memory yourself
	_ifc.ifc_buf = (char *)malloc( _ifc.ifc_len );
	bzero( _ifc.ifc_buf, _ifc.ifc_len );
	ret = ioctl( _sock, SIOCGIFCONF, &_ifc, sizeof(_ifc) ); // get ifr_name
	if (ret != 0) goto out;
	//assert( !ret );

		n = _ifc.ifc_len / sizeof(struct ifreq);//you have n net devices
	pifr = (struct ifreq *)_ifc.ifc_buf;

	for ( i = 0; i < n; ++i, ++pifr ) 
	{
		//CheckDevice( pifr );
		if ( pifr->ifr_addr.sa_family != PF_INET ) continue;	// not tcp/ip
		bzero( &_device, sizeof(_device) );
		bzero( &_ifr, sizeof(_ifr) );
		strcpy( _ifr.ifr_name, pifr->ifr_name );
	
		int	ret = ioctl( _sock, SIOCGIFHWADDR, &_ifr, sizeof(_ifr) );
		if (ret != 0) continue;

		if ( _ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER ) continue; // not ethernet
		memcpy( _device.mac, _ifr.ifr_hwaddr.sa_data, 6 );

		// get ip address
		ret = ioctl( _sock, SIOCGIFADDR, &_ifr, sizeof(_ifr) );
		if (ret != 0) continue;

		memcpy( &_device.ip, _ifr.ifr_addr.sa_data, 4 );
		_device.ip = ((struct sockaddr_in *)(&_ifr.ifr_addr))->sin_addr.s_addr;

		// get ip sub-net mask
		ret = ioctl( _sock, SIOCGIFNETMASK, &_ifr, sizeof(_ifr) );
		if (ret != 0) continue;

		_device.netmask = ((struct sockaddr_in *)(&_ifr.ifr_netmask))->sin_addr.s_addr;

		// copy devie name
		strncpy( _device.name, _ifr.ifr_name, sizeof(_device.name)-1 );
		memcpy (list + nRet, &_device, sizeof(nic_t));
		nRet++;
	}
out:
	free( _ifc.ifc_buf);
	close(_sock);
	return nRet;

}
#endif
	
	
#ifdef _WIN32
int	GetNetip(unsigned int *ip)
{
		
int i = 0;
	char szhn[256]; 
	int nStatus = gethostname(szhn, sizeof(szhn)); 
	HOSTENT *host =0;
	if (nStatus == -1 ) 
	{ 
			perror("gethostname failed\n"); 
			return 0; 
	} 
		
	host = gethostbyname(szhn); 
		
	if (host != NULL) 	
	{ 
			
		for (i=0; ; i++ ) 		
		{ 
				//cout <<" "<<inet_ntoa(*(IN_ADDR*)(host->h_addr_list[i]))<<endl;
		   ip[i] =  *(unsigned long*)host->h_addr_list[i];	
		   if ( host->h_addr_list[i] + host->h_length >= host->h_name ) 
			break; 
		} 
			
	}  
		return i;		
}
#endif

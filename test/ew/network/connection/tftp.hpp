#pragma once

// tftp_client
int tftp_client(char * host, char * port);

#if 0
// TODO
int tftp_revcfile(tftp_connection * conn,
		  const char * filename,
		  const size_t filename_len,
		  ew::core::objects::write_interface * out);
#endif

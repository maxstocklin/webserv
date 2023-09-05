/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstockli <mstockli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:53:09 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/04 15:44:41 by mstockli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

#include "BindingSocket.hpp"

class ListeningSocket : public BindingSocket
{
	public:
		ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int bklog);
		void start_listening();
		
	
	private:
		int backlog;
		int	listening;

		ListeningSocket();
};
#endif
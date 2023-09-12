/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: max <max@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 14:53:09 by mstockli          #+#    #+#             */
/*   Updated: 2023/09/10 23:07:09 by max              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

#include "Colors.hpp"

#include "BindingSocket.hpp"

class ListeningSocket : public BindingSocket
{
	public:
		ListeningSocket(std::string serverBlock);

		void start_listening();
		
		
	private:
		int							backlog;					// hardcoded
		int							listening;

		ListeningSocket();
};
#endif
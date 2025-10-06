/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppeckham <ppeckham@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:09:46 by ppeckham          #+#    #+#             */
/*   Updated: 2025/10/06 13:09:47 by ppeckham         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "server.hpp"
# include "channel.hpp"

class	Client
{

private:
	int					_sock_fd;
	//int				_port;
	sockaddr_in			_address;
	std::string 		_nick;
	std::string 		_user;
	std::string 		_real_name;
	bool				_isNick_set;
	bool				_isUser_set;
	bool				_isAuthenticated;
	bool				_isRegistered;
	std::list<Channel*>	_channels;
	std::string			_message_buffer;

	Client( const Client& other );
	Client& operator=( const Client& other );

public:
	Client( void );
	~Client();

	Client( int fd, sockaddr_in address );

	void				set_nick( std::string& nickname );
	void				set_user( std::string& username );
	void				set_real_name( std::string& realname );
	void				set_authenticated_status( bool status );
	void				set_register_status( bool status );
	bool				isAuthenticated( void ) const;
	bool				isRegistered( void ) const;
	int					get_sock_fd( void );
	std::string 		get_nick( void );
	std::string 		get_user( void );
	std::string 		get_real_name( void );
	void				addChannel( Channel* Channel, const std::string& key );
	void				removeChannel( Channel* channel );
	bool				isInChannel( Channel* channel ) const;
	std::list<Channel*>	getChannels( void );
	void				appendToBuffer( const std::string& data );
	std::string			extractCompleteMessage( void );

};

#endif

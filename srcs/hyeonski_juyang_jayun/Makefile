SRCNAME	=		\
					main.cpp\
					Client.cpp\
					Location.cpp\
					Manager.cpp\
					Server.cpp\
					Webserver.cpp\
					Request.cpp\
					Response.cpp\
					Type.cpp\
					CGI.cpp

SRCDIR		=		./srcs/

SRCS		=		${addprefix ${SRCDIR}, ${SRCNAME}}

INC			=		-I ./includes/ -I /libft_cpp/

NAME		=		webserv

LIB_NAME	=		libft.a

CC			=		clang++

CF			=		-Wall -Wextra -Werror -std=c++98 ${INC} ${SRCS}
DCF			=		-g ${SRCS}

${NAME}     :
					make all -C "./libft_cpp"
					cp libft_cpp/${LIB_NAME} ${LIB_NAME}
					${CC} ${CF} ${LIB_NAME} ${INC} -o ${NAME} 

test		:
					${CC} ${DCF} ${LIB_NAME} ${INC} -o ${NAME}
					rm -rf ./tests/put_test/file_should_exist_after
					./webserv configs/test.conf

fclean		:
					make fclean -C "./libft_cpp"
					rm -rf webserv.dSYM
					rm -rf ${NAME}
					rm -rf ${LIB_NAME}

re			:		fclean all

all         :      	${NAME}

.PHONY		:		fclean re test

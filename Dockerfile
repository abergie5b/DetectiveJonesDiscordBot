# our local base image
FROM ubuntu 

LABEL description="KaliDiscordBot"

ENV DEBIAN_FRONTEND=noninteractive

# install build dependencies 
RUN apt-get update && apt-get install -y g++ rsync zip openssh-server make  cmake git 
RUN apt-get install libcurl4-openssl-dev libssl-dev \
   && echo 'openssl_conf = default_conf' > /etc/ssl/openssl.cnf \
   &&  echo '[ default_conf ]' >> /etc/ssl/openssl.cnf \
   &&  echo 'ssl_conf = ssl_sect' >> /etc/ssl/openssl.cnf \
   &&  echo '[ssl_sect]' >> /etc/ssl/openssl.cnf \
   &&  echo 'system_default = system_default_sect' >> /etc/ssl/openssl.cnf \
   &&  echo '[system_default_sect]' >> /etc/ssl/openssl.cnf \
   &&  echo 'MinProtocol = TLSv1.2' >> /etc/ssl/openssl.cnf \
   &&  echo 'CipherString = DEFAULT:@SECLEVEL=1' >> /etc/ssl/openssl.cnf 

# configure SSH for communication with Visual Studio 
RUN mkdir -p /var/run/sshd

RUN echo 'PasswordAuthentication yes' >> /etc/ssh/sshd_config && \ 
   ssh-keygen -A 

# expose port 22 
EXPOSE 22

#service ssh start
#useradd -m -d /home/kali -s /bin/bash -G sudo kali
#passwd kali


# Adding SSL Encryption to Your Webserver

## Getting started
+ Install OpenSSL on your system. There are OpenSSL install packages for all major Linux distributions.
+ Create a SSL certificate
+ Edit your input file

## Creating a SSL certificate
You can use your own certificate but to create a self signed certificate follow the steps below:
```
  openssl genrsa -des3 -out server.key 1024

  openssl req -new -key server.key -out server.csr

  cp server.key server.key.orig

  openssl rsa -in server.key.orig -out server.key

  openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt

  cp server.crt server.pem

  cat server.key >> server.pem
```

## Edit your input file

add the following lines to your input file
```python
web.server.ssl_enable = True
web.server.path_to_ssl_cert = "server.pem"
```
Where server.pem is the path to the server.pem file you created when creating a SSL certificate

## Access your webserver
Now that ssl encryption is enabled, to access you webserver use https://localhost.ssl:8888 instead of http and wss://localhost.ssl:8888 for http and ws protocals respectively.

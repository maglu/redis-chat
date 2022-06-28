# redis-chat
This is a simple chat I used to learn and test some features of Redis (https://redis.io/) in C++.

The application will create a new chat using a redis stream _chatName_.chat that will have all chat messages and a pub/sub channel called _chatName_.cmd that will publish events like user joining/leaving the chat.

## Usage
The application will start in the mode "listener" or "writer" first will receive all the chat messages and events and second will send messages. To create a listener one needs to pass as parameter only the chat name and to create a writer we need a chat name and a user name - see below. We can have as many listeners and writers as we want, limited only by redis server capacity.

Create listener
```
./chat camelot
```

Create writers
```
./chat camelot arthur
```

```
./chat camelot patsy
```

When the writers join the chat listener will On each writer you can send messages and they will be shown on  
## Prerequisites
This chat uses redis-plus-plus library (https://github.com/sewenew/redis-plus-plus) which requires hiredis (https://github.com/redis/hiredis)

The application is hardcoded to connect on a local Redis server, so you need to install the redis server on your local machine or change the setup on chat.h.

Simplified installation steps for Ubuntu, for different environments refers to the libs page.

### Install Libraries

```
sudo apt-get update

sudo apt-get install libhiredis-dev

git clone https://github.com/sewenew/redis-plus-plus.git
cd redis-plus-plus
mkdir build
cd build
cmake -DREDIS_PLUS_PLUS_CXX_STANDARD=17 ..
make
make install

```

### Install redis
Reference https://redis.io/docs/getting-started/installation/install-redis-on-linux/

```
curl -fsSL https://packages.redis.io/gpg | sudo gpg --dearmor -o /usr/share/keyrings/redis-archive-keyring.gpg

echo "deb [signed-by=/usr/share/keyrings/redis-archive-keyring.gpg] https://packages.redis.io/deb $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/redis.list

sudo apt-get install redis
````

## Build application

```
git clone https://github.com/maglu/redis-chat.git
cd redis-chat
mkdir build
cd build
cmake ..
make
```


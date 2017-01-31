FROM ubuntu:16.04
MAINTAINER Frank Celler <frank@arangodb.com>

# add mesos framework files
ADD ./memtest.sh /memtest.sh
ADD ./memtest /memtest

# start script
ENTRYPOINT ["/memtest.sh"]
CMD ["memtest"]

FROM alpine:latest

RUN apk update && apk upgrade && apk add build-base

WORKDIR /src 
CMD [ "make",  "build" ]


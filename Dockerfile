FROM node:lts-bullseye

RUN mkdir /workspace
WORKDIR /workspace

COPY . /workspace

RUN npm install && npm run prebuild

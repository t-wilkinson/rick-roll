# rick-roll

Projects involving rick-rolls. `rick-roll-detector` detects if a website is a rick-roll attempt. `rick-roll-virus` is a program that injects itself into ip tables and intercepts tcp requests, randomly replacing http requests with a redirect to "Never Gonna Give You Up".

**Projects:**

- [rick-roll-virus](./rick-roll-virus)
- [rick-roll-detector](./rick-roll-detector)

The rick-roll-virus features ip tables.
The rick-roll-detector features a key-value store web API.

## Key Features

- NetEdit: generally useful program to intercept tcp requests

## Technologies Used

- iptables
- Languages and frameworks
- Platforms and services
- Deployment details

## Installation and Setup

## Usage Examples

**rick-roll-detector**

```bash
bash ./rick-roll-detector/scripts/start.sh
```

will compile and run the database and webserver

**rick-roll-virus**

```bash
cd ./rick-roll-detector
bash ./scripts/iptables.sh start
cd netedit
cargo build
target/debug/netedit tcp 11110
```

## Architecture Overviefw

**rick-roll-detector**

- Website
  - Simple HTML file.
  - Interfaces with webserver through simple http requests.
  - User can enter url and get a response detailing if the link is a rick roll.
- Webserver
  - Written in Go.
  - Returns the webpage at `/` or receives a url to detect at `/search?query=<query>`
- Rick roll detector service
  - Written in Go.
  - A go service running a rpc server. Way over the top.
- In-memory database
  - Written in C.
  - Caches search queries.
  - Periodically saves database to filesystem.
  - Interacts through TCP.

**rick-roll-virus**

-

## Testing

## Challenges and Learnings

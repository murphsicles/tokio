# @async/tokio — Full Async Runtime for Zeta

Auto-converted from [tokio](https://crates.io/crates/tokio) v1.52.3 via [Dark Factory](https://github.com/murphsicles/dark-factory).

## Usage
```zeta
#[tokio::main]
async fn main() {
    let stream = TcpStream::connect("127.0.0.1:8080").await.unwrap();
    stream.write_all(b"hello").await.unwrap();
}
```

## Stats
- 373 source files, ~36,227 lines of Zeta
- 0 unsupported items
- Previous hand-written version preserved in `legacy/`

## License
MIT

### dvid server

## run dvid
nohup dvid -verbose serve $BUILDEM_DIR/config/config-august.toml

## shutdown
dvid -rpc=:7401 shutdown


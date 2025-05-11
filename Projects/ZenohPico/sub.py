import time
import zenoh
import struct

def listener(sample: zenoh.Sample):
    print(struct.unpack("ffffff", bytes(sample.payload)))

conf = zenoh.Config()
zenoh.init_log_from_env_or("error")

with zenoh.open(conf) as session:
    session.declare_subscriber("demo/example/**", listener)
    print("[Subscriber] Waiting for packages")

    # Keep alive
    while True:
        time.sleep(1)

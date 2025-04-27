import posix_ipc
import mmap
import struct

class Publisher:
    def __init__(self, topic_name, fmt):
        """
        topic_name: str        - Name of the topic
        fmt: struct format str - Format of the message (example: 'if' for int+float)
        """
        self.fmt = fmt
        self.size = struct.calcsize(fmt)
        shm_name = f"/{topic_name}_shm"
        sem_name = f"/{topic_name}_sem"

        self.shm = posix_ipc.SharedMemory(shm_name, posix_ipc.O_CREAT, size=self.size)
        self.map_file = mmap.mmap(self.shm.fd, self.size)
        self.shm.close_fd()

        self.sem = posix_ipc.Semaphore(sem_name, posix_ipc.O_CREAT, initial_value=0)

    def publish(self, *values):
        packed = struct.pack(self.fmt, *values)
        self.map_file.seek(0)
        self.map_file.write(packed)
        self.map_file.flush()
        self.sem.release()

    def close(self):
        self.map_file.close()
        self.sem.close()

class Subscriber:
    def __init__(self, topic_name, fmt):
        """
        topic_name: str        - Name of the topic
        fmt: struct format str - Format of the message (example: 'if' for int+float)
        """
        self.fmt = fmt
        self.size = struct.calcsize(fmt)
        shm_name = f"/{topic_name}_shm"
        sem_name = f"/{topic_name}_sem"

        self.shm = posix_ipc.SharedMemory(shm_name)
        self.map_file = mmap.mmap(self.shm.fd, self.size, mmap.MAP_SHARED, mmap.PROT_READ)
        self.shm.close_fd()

        self.sem = posix_ipc.Semaphore(sem_name)

        self._flush_semaphore()

    def _flush_semaphore(self):
        try:
            while True:
                self.sem.acquire(timeout=0)
        except posix_ipc.BusyError:
            pass

    def receive(self):
        # Blocking
        self.sem.acquire()
        self.map_file.seek(0)
        data = self.map_file.read(self.size)
        return struct.unpack(self.fmt, data)
    
    def check(self):
        try:
            self.sem.acquire(0)
            self.map_file.seek(0)
            data = self.map_file.read(self.size)
            return (1, struct.unpack(self.fmt, data))
        except posix_ipc.BusyError:
            return (0, None)

    def close(self):
        self.map_file.close()
        self.sem.close()

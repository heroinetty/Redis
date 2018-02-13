import redis

class Database:
    def __init__(self):
        self.host = 'localhost'
        self.port = 6379
    def write(self, website, city, year, month, day, deal_number):
        try:
            key = '_'.join([website, city, str(year), str(month), str(day)])
            val = deal_number
            r = redis.StrictRedis(host = self.host, port = self.port)
            r.set(key, val)
            return 
        except Exception as exception:
            print(exception)
    def read(self, website, city, year, month, day):
        try:
            key = '_'.join([website, city, str(year), str(month), str(day)])
            r = redis.StrictRedis(host = self.host, port = self.port)
            value = r.get(key)
            return value
        except Exception as exception:
            print(exception)
if __name__ == "__main__":
    db = Database()
    db.write('itcastcpp', 'beijing', 2016, 1, 22, 8000)
    print(db.read('itcastcpp', 'beijing', 2016, 1, 22))
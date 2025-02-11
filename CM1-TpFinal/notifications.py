
class VerboseLogger:
    def __init__(self):
        pass

    def log(self, s):
        print(s)

    def notify_layout_started(self, vertex_count, edge_count):
        print("Performing layout on", vertex_count, "vertices, and", edge_count, "edges")

    def notify_layout_completed(self):
        print("Layout completed")

    def notify_zero_temperature(self):
        print("Temperature too low, the graph freezed.")

class QuietLogger:
    def __init__(self):
        pass

    def log(self, s):
        pass

    def notify_layout_started(self, vertex_count, edge_count):
        pass

    def notify_layout_completed(self):
        pass

    def notify_zero_temperature(self):
        pass 
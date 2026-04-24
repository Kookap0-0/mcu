import time
import serial
import matplotlib.pyplot as plt

def read_value(ser):
    """Читает строку, преобразует в число с плавающей точкой."""
    while True:
        try:
            line = ser.readline().decode('ascii').strip()
            value = float(line)
            return value
        except ValueError:
            continue

def main():
    port_name = '/dev/ttyACM0'

    ser = serial.Serial(port=port_name, baudrate=115200, timeout=0.0)

    if ser.is_open:
        print(f"Port {ser.name} opened")
    else:
        print(f"Port {ser.name} closed")
        return

    measure_temperature_C = []
    measure_voltage_V = []
    measure_ts = []

    start_ts = time.time()

    try:
        while True:
            ts = time.time() - start_ts

            ser.write(b"get_adc\n")
            voltage_V = read_value(ser)

            # Команда get_temp
            ser.write(b"get_temp\n")
            temp_C = read_value(ser)

            measure_ts.append(ts)
            measure_voltage_V.append(voltage_V)
            measure_temperature_C.append(temp_C)

            print(f'{voltage_V:.3f} V - {temp_C:.1f} C - {ts:.2f} s')

            time.sleep(0.1)

    except KeyboardInterrupt:
        print("\nПрерывание пользователем.")
    finally:
        ser.close()
        print("Port closed")

        plt.figure(figsize=(10, 8))

        plt.subplot(2, 1, 1)
        plt.plot(measure_ts, measure_voltage_V, 'b-', linewidth=2)
        plt.title('График зависимости напряжения от времени')
        plt.xlabel('время, с')
        plt.ylabel('напряжение, В')
        plt.grid(True)

        plt.subplot(2, 1, 2)
        plt.plot(measure_ts, measure_temperature_C, 'r-', linewidth=2)
        plt.title('График зависимости температуры от времени')
        plt.xlabel('время, с')
        plt.ylabel('температура, °C')
        plt.grid(True)

        plt.tight_layout()
        plt.show()

if __name__ == "__main__":
    main()
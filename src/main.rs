use serialport::SerialPort;
use std::{io::Write, thread, time::Duration};
use sysinfo::{Components, System};

fn main() {
    let mut sys = System::new_all();
    let mut comps = Components::new();

    let mut port = serialport::new("/dev/ttyACM0", 9600)
        .timeout(Duration::from_millis(100))
        .open()
        .unwrap();

    loop {
        sys.refresh_cpu_all();
        sys.refresh_memory();
        comps.refresh(false);

        let cpu = sys.global_cpu_usage() as u8;
        let mem = (sys.used_memory() * 100 / sys.total_memory()) as u8;

        let temp = std::fs::read_to_string("/sys/class/thermal/thermal_zone0/temp")
            .ok()
            .and_then(|s| s.trim().parse::<i32>().ok())
            .map(|t| (t / 1000) as u8)
            .unwrap_or(0);

        let out = format!("CPU {:02} MEM {:02}\nTMP {:02}\n", cpu, mem, temp);

        port.write_all(out.as_bytes()).unwrap();
        thread::sleep(Duration::from_secs(1));
    }
}

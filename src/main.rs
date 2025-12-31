use std::{io::Write, thread, time::Duration};
use sysinfo::{Components, System};

fn read_gpu_busy() -> u8 {
    std::fs::read_dir("/sys/class/drm")
        .ok()
        .and_then(|dirs| {
            for d in dirs.flatten() {
                let p = d.path().join("device/gpu_busy_percent");
                if let Ok(s) = std::fs::read_to_string(p) {
                    if let Ok(v) = s.trim().parse::<u8>() {
                        return Some(v);
                    }
                }
            }
            None
        })
        .unwrap_or(0)
}

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
        let gpu = read_gpu_busy();

        let temp = std::fs::read_to_string("/sys/class/thermal/thermal_zone0/temp")
            .ok()
            .and_then(|s| s.trim().parse::<i32>().ok())
            .map(|t| (t / 1000) as u8)
            .unwrap_or(0);

        let out = format!(
            "CPU {:03} MEM {:03} TMP {:03} GPU {:03};",
            cpu, mem, temp, gpu
        );

        println!("{out}");

        port.write_all(out.as_bytes()).unwrap();
        thread::sleep(Duration::from_secs(1));
    }
}

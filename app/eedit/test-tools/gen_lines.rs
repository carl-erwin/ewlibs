use std::os;

fn main()
{
  let os_args = os::args();
  let args    = os_args.as_slice();

  if args.len() != 4 {
    println!("usage : {} start stop width", args[0]);
    return;
  }

  let start_num = args[1].trim_right().parse::<u64>().unwrap_or(0);
  let stop_num  = args[2].trim_right().parse::<u64>().unwrap_or(0);
  let width_num = args[3].trim_right().parse::<u64>().unwrap_or(0);

  gen_lines(start_num, stop_num, width_num);
}

fn gen_lines(start: u64, stop: u64, linewidth: u64) -> ()
{
  let string = gen_line(linewidth);

  for x in range(start, stop) {
    print!("{} {}", x + 1, string);
  }
}



fn gen_line(linewidth : u64) -> String
{
  let mut string = String::new();

  for x in range(0, linewidth) {
    string.push('x');
  }
  string.push('\n');
  return string;
}


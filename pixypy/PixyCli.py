from .Pixy import Pixy
import click, sys, re
from serial.tools import list_ports

pixy = Pixy()

@click.group(invoke_without_command=True)
@click.pass_context
def move(ctx):
    if ctx.invoked_subcommand is None:
        if(ctx.obj and ctx.obj['cmd_pieces'] and len(ctx.obj['cmd_pieces'])):
            cmd = globals()[ctx.obj['cmd_pieces'].pop(0)]
            params = {}
            for idx, arg in enumerate(cmd.params):
                params[arg.name] = ctx.obj['cmd_pieces'][idx]
            ctx.invoke(cmd, **params)
        else:
            move('help')


@click.command()
def tty():
    ports = sorted(list_ports.comports(include_links=True))
    opts = {}
    if(len(ports)):
        click.echo(
            "Please select the port you wish to send pixy data over from the list below"
        )
        click.echo(f"{'':<5} {'Port':^10} {'Description':^30} {'HWID':^10}")
        click.echo('-----------------------------------------------------')
        for idx, [port, desc, hwid] in enumerate(ports):
            click.echo(
                f"{f'({idx})':<5}{port:^5} {desc:^20} {f'[{hwid}]':^21}")

        opts['port'] = click.prompt(
            "Port", 0, value_proc=(lambda port: ports[int(port)][0]))
        opts['coords'] = click.prompt("Starting Coordinates x,y", '0,0', value_proc=(
            lambda xy: [int(xy.split(',')[0]), int(xy.split(',')[1])]))
        opts['steps'] = click.prompt('Default Number of Steps Per Transition',10)
        opts['easing_fn'] = click.prompt('Default Easing Function', 'CubicEaseIn')
        opts['baud_rate'] = click.prompt('Baud Rate:', 9600)
        pixy.begin(opts)
        await_command()

    else:
        click.echo("There are no com ports available on your system!")
        exit(1)

def await_command():
    cmd : string = click.prompt("Enter a command")
    pieces = list(filter((lambda s: s != '' and not re.search("^\\s+$", s)), cmd.split(' ')))
    group = pieces.pop(0)
    obj = {"cmd_pieces": pieces, "group": group}
    move(obj=obj)
        
    
def is_string(O):
    return hasattr(O, 'upper') and callable(O.upper)

@move.command()
@click.argument('amt')
def left(amt):
    '''Send the pixy commands corresponding to moving the tracked object left by the given amount'''
    pixy.move_left(amt)

@move.command()
@click.argument('amt')
def right(amt):
    '''Send the pixy commands corresponding to moving the tracked object right by the given amount'''
    pixy.move_right(amt)

@move.command()
@click.argument('amt')
def up(amt):
    '''Send the pixy commands corresponding to moving the tracked object up by the given amount'''
    pixy.move_up(amt)

@move.command()
@click.argument('amt')
def down(amt):
    '''Send the pixy commands corresponding to moving the tracked object down by the given amount'''
    pixy(amt)

@move.command()
@click.argument('x')
@click.argument('y')
def to(x, y):
    '''Send the pixy commands corresponding to moving the tracked object to the destination via a straight line'''
    pixy.go_to(x, y)

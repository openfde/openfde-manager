package security

import (
	"os"
	"os/exec"
)

type UosSecurity struct {
}

var uosCmdList = [][]string{
	{"sed", "-i", "s/1/0/", "/usr/share/deepin-elf-veirfy/mode"},
	{"systemctl", "restart", "deepin-elf-verify"},
}

func (impl UosSecurity) Disable() error {
	for _, cmdString := range uosCmdList {
		cmd := exec.Command(cmdString[0], cmdString[1:]...)
		if err := cmd.Run(); err != nil {
			return err
		}
	}
	return nil
}

func (impl UosSecurity) Query() bool {
	data, err := os.ReadFile("/usr/share/deepin-elf-verify/mode")
	if err != nil {
		return false
	}
	return string(data) == "1"
}

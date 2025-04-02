package security

import (
	"os/exec"
	"strings"
)

type KylinSecurity struct {
}

var kylinCmdList = [][]string{
	{"setsignstatus", "off"},
	{"setstatus", "softmode"},
	{"setstatus", "-f", "exectl", "off", "-p"},
	{"setstatus", "-f", "netctl", "off", "-p"},
	{"setstatus", "-f", "devctl", "off", "-p"},
	{"setstatus", "-f", "ipt", "off", "-p"},
	{"setstatus", "-f", "fpro", "off", "-p"},
	{"setstatus", "-f", "ppro", "off", "-p"},
	{"setstatus", "-f", "kmod", "off", "-p"},
	{"sed", "-i", "/s/kysec_exectl.*/kysec_exectl = 0/", "/etc/kysec/kysec.conf"},
	{"sed", "-i", "/s/kysec_netctl.*/kysec_netctl = 0/", "/etc/kysec/kysec.conf"},
	{"sed", "-i", "/s/kysec_devctl.*/kysec_devctl = 0/", "/etc/kysec/kysec.conf"},
	{"sed", "-i", "/s/kysec_ipt.*/kysec_ipt = 0/", "/etc/kysec/kysec.conf"},
	{"sed", "-i", "/s/kysec_fpro.*/kysec_fpro = 0/", "/etc/kysec/kysec.conf"},
	{"sed", "-i", "/s/kysec_ppro.*/kysec_ppro = 0/", "/etc/kysec/kysec.conf"},
	{"sed", "-i", "/s/kysec_kmodpro.*/kysec_kmodpro = 0/", "/etc/kysec/kysec.conf"},
}

func (impl KylinSecurity) Disable() error {
	for _, cmdString := range kylinCmdList {
		cmd := exec.Command(cmdString[0], cmdString[1:]...)
		if err := cmd.Run(); err != nil {
			return err
		}
	}
	return nil
}

func (impl KylinSecurity) Query() bool {
	cmd := exec.Command("getstatus")
	output, err := cmd.Output()
	if err != nil {
		return false
	}

	lines := string(output)
	if execStatus := parseStatus(lines, "exec"); execStatus != "off" {
		return true
	}
	return false
}

func parseStatus(output, key string) string {
	for _, line := range strings.Split(output, "\n") {
		if strings.Contains(line, key) {
			parts := strings.Split(line, ":")
			if len(parts) > 1 {
				return strings.TrimSpace(parts[len(parts)-1])
			}
		}
	}
	return ""
}

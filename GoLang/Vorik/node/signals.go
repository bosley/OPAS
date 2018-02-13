package node

import (
	"os"
	"log"
	"os/signal"
	"syscall"
)

func NodeSignalIntercept() chan int{
	sigchan := make(chan os.Signal, 1)
	signal.Notify(sigchan,
		syscall.SIGHUP,
		syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT)
	outchan := make(chan int)

	go func() {
		for {
			s := <-sigchan
			switch s {
			case syscall.SIGHUP:
				log.Println("NodeSignalIntercept() -> hungup")
				outchan <- 3
				
			case syscall.SIGINT:
				log.Println("NodeSignalIntercept() -> interrupt")
				outchan <- 2

			case syscall.SIGTERM:
				log.Println("NodeSignalIntercept() -> force stop")
				outchan <- 0
				
			case syscall.SIGQUIT:
				log.Println("NodeSignalIntercept() -> stop, core dump")
				outchan <- 0
				
			default:
				log.Println("NodeSignalIntercept() -> unknown signal.")
				outchan <- 1
			}
		}
	}()
	return outchan
}
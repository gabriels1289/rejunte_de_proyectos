import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { AuthService } from 'src/app/auth/auth.service';

@Component({
  selector: 'app-menu',
  templateUrl: './menu.component.html',
  styleUrls: ['./menu.component.css']
})
export class MenuComponent implements OnInit {
  private idevaluacion: any;
  public titulo: any;

  constructor(
    private rutaActiva: ActivatedRoute,
    private router: Router,
    private authService: AuthService
  ) {
    this.idevaluacion = this.rutaActiva.snapshot.params.idevaluacion;
    this.titulo = this.rutaActiva.snapshot.params.titulo;
  }

  ngOnInit() {
  }

}

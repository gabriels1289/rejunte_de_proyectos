import { Component, OnInit } from '@angular/core';
import { Router, NavigationExtras, ActivatedRoute } from '@angular/router';
import { AuthService } from '../auth.service';

import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { first } from 'rxjs/operators';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {

  message: string;

  // esto lo agregue yo para crear el formulario
  loginForm: FormGroup;
  loading = false;
  submitted = false;
  returnUrl: string;
  error = '';

  constructor(
    public authService: AuthService,
    private route: ActivatedRoute,
    public router: Router,
    private formBuilder: FormBuilder
    ) {
      this.setMessage();

      // Si la ruta es de deslogueo invoco a la funcion
      if (this.router.url === '/logout') {
        this.authService.logout();
      }

      // redirect to home if already logged in
      if (this.authService.currentUserValue) {
        // console.log('en el IF del constructor del login');
        this.router.navigate(['/']);
      }
  }

  setMessage() {
    this.message = 'Logged ' + (this.authService.isLoggedIn ? 'in' : 'out');
  }

  login() {
    this.message = 'Trying to log in ...';

   /* this.authService.login().subscribe(() => {
      this.setMessage();
      if (this.authService.isLoggedIn) {
        // Get the redirect URL from our auth service
        // If no redirect has been set, use the default
        // uso const, pero era let
        const redirect = this.authService.redirectUrl ? this.router.parseUrl(this.authService.redirectUrl) : '/admin';

        // Set our navigation extras object (era let)
        // that passes on our global query params and fragment
        const navigationExtras: NavigationExtras = {
          queryParamsHandling: 'preserve',
          preserveFragment: true
        };

        // Redirect the user
        this.router.navigateByUrl(redirect, navigationExtras);
      }
    });*/
    this.authService.login(this.f.username.value, this.f.password.value)
    .pipe(first())
    .subscribe(
        data => {
            // console.log('en el data del login');
            // Get the redirect URL from our auth service
            // If no redirect has been set, use the default
            // const redirect = this.authService.redirectUrl ? this.router.parseUrl(this.authService.redirectUrl) : '/admin';
            this.router.navigate([this.returnUrl]);
            // this.router.navigateByUrl(redirect);
        },
        error => {
            console.log('error login: %o', error);
            this.error = error;
            this.loading = false;
        });
  }

  logout() {
    this.authService.logout();
    this.setMessage();
  }

  // este metodo estaba vacio
  ngOnInit() {
    this.loginForm = this.formBuilder.group({
      username: ['', Validators.required],
      password: ['', Validators.required]
    });

    // get return url from route parameters or default to '/'
    // tslint:disable-next-line: no-string-literal
    this.returnUrl = this.route.snapshot.queryParams['returnUrl'] || '/';
  }

  // convenience getter for easy access to form fields
  get f() { return this.loginForm.controls; }

  onSubmit() {
    this.submitted = true;

    // stop here is form is invalid
    if (this.loginForm.invalid) {
      return;
    }

    this.loading = true;

    // aca iria la logica con el servicio de authentication
    this.login();
  }
}
